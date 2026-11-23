# KNOWN_BUGS.md — реестр багов

Проект: threader (threader.std, threader.ssl)
Последнее обновление: 2026-11-24 (пятая итерация ревью)

---

## Сводка

| Статус | Баги |
|---|---|
| FIXED (редизайн класса) | BUG-1, BUG-2, BUG-3, BUG-6 |
| FIXED (2-я итерация) | BUG-7 (частично, см. NOTE N1) |
| FIXED (3-я итерация) | BUG-9, BUG-10 (linux, частично), восстановление NB-режима клиента |
| FIXED (4-я итерация) | BUG-11, BUG-13 (Windows-ветка) |
| FIXED (5-я итерация) | BUG-14 |
| OPEN | BUG-15, BUG-12, BUG-13 (остаток, linux), BUG-8, BUG-5, BUG-4 |
| VERIFY (нужна проверка на полной версии кода) | см. раздел VERIFY |

---

## Исправленные баги

### Исправлено редизайном (пер-экземплярный контекст)

Статические `SSL_CTX`/`SSL` и счётчики ссылок заменены полями экземпляра
(`_sslContext`, `_sslInstance`). Подтверждено хендшейк-тестом.

- **BUG-1** — `static bool _useExtrenalCerts` затирался в конструкторе каждого
  экземпляра. Стал полем экземпляра; регрессионный тест
  `SslSocket.ExternalCertsFlagIsPerInstance` (активный).
- **BUG-2** — гонка на статических контекстах и счётчиках. Статики устранены.
- **BUG-3** — двойной декремент счётчика (`close()` + деструктор), освобождение
  чужого контекста при разрушении одного из сокетов. Декрементов больше нет.
- **BUG-6** — `close()` не сбрасывал состояние в `Disconnected`; деструктор
  звал `close()` повторно → `SSL_shutdown(nullptr)`, сегфолт (подтверждено
  падением интеграционного теста на разборке). Теперь `close()` делегирует
  только `HandlerTcpSocket::close()`, SSL-объекты освобождаются однократно
  в `sslFinalize()` из деструктора.

### Исправлено на 2-й итерации

- **BUG-7** — `initialized` никогда не выставлялся: инициализация OpenSSL
  выполнялась при каждом создании сокета. Добавлено `initialized = true;`
  и `std::call_once`. Остаток — NOTE N1 (гонка на неатомарном флаге).

### Исправлено на 3-й итерации

- **BUG-9** — при `verifyCertificate == true` обе платформы выставляли
  `SSL_VERIFY_NONE`: проверка была no-op, `SSL_get_verify_result` всегда
  возвращал `X509_V_OK` (ложное чувство защищённости). Теперь обе ветки
  выставляют `SSL_VERIFY_PEER`; на Windows `SSL_CTX_load_verify_locations`
  вызывается до хендшейка, файл CA проверяется на существование.
- **BUG-10 (linux, частично)** — temp-файлы сертификатов создаются через
  `mkstemp` с шаблонами `XXXXXX` — уникальность имен восстановлена.
  Остатки — NOTE N3, N4. На WIN32 ветка по-прежнему не компилируется (BUG-12).
- Восстановление неблокирующего режима после хендшейка клиента —
  `setBlockingMode(d, false)` в `process()` (linux).

### Исправлено на 4-й итерации

- **BUG-11** — `assign()` (linux) делал блокирующий `SSL_accept` без ожидания
  ClientHello: клиент с задержкой получал отказ, молчаливый клиент подвешивал
  поток голосования. Теперь хендшейк асинхронный: первая попытка без ожидания,
  шаги выполняет `process()` по событию `POLLIN`, `WANT_READ/WANT_WRITE` —
  норма ожидания, лимит попыток `_acceptCounter` = `PROCESS_ACCEPT_COUT`
  против «вечно недокрученного хендшейка». POLLOUT выдаётся только клиенту
  в `Connecting` (иначе — цикл на 100% CPU).
- **BUG-13 (Windows)** — `openTcpClientSocket` использует `1 != ret` и
  `SSL_get_error` вместо проверки булева результата и `lastError()`.

### Исправлено на 5-й итерации (текущая)

- **BUG-14** — `_acceptCounter` не инициализировался на linux (инициализация
  оставалась под `#ifdef WIN32` при общем поле): мусорное значение давало
  ложные срабатывания таймаута хендшейка сервера. Теперь `_acceptCounter(0)`
  вынесен из ifdef и в хедере, и в конструкторе. Закрыто.

---

## Открытые баги (по приоритету)

### BUG-15: `assign()` регистрирует мёртвый сокет при провале подготовки

**Файл:** `HandlerSslSocket.cpp`, `assign()` (linux). **Статус: OPEN (не исправлен).**

Локальная копия `connectionStateLocal` снимается до ветки ошибок. Ветки ошибок
(`!_sslInstance`, `!setInstanceDescriptor()`, провал `setBlockingMode`)
ставят состояние `Disconnected`, но хвост функции проверяет устаревшую
локальную копию — мёртвый дескриптор попадает в массив голосования. Условие
POLLOUT тоже смотрит на устаревшее состояние.

**Фикс:** перечитать `connectionState()` в хвосте:

```cpp
auto currentState = connectionState(); // свежее значение
if (currentState == ConnectionState::Disconnected)
    return;
event.events = POLLIN | POLLERR | POLLHUP | POLLNVAL | POLLRDHUP;
if ((currentState == ConnectionState::Connecting &&
     socketType() == TcpSocketType::Client) || needsToWrite())
    event.events |= POLLOUT;
```

### BUG-12: temp-блок `sslInitialize()` не компилируется под Windows

**Файл:** `HandlerSslSocket.cpp`, `sslInitialize()`. **Статус: OPEN.**

`certificateFileName`/`privateKeyFilename` объявлены внутри `#ifdef linux`,
а присваивания `_certificateFilename = certificateFileName;` стоят после
`#endif` — на Windows переменных не существует, сборка упадёт. Прежний
механизм (`certifaceXXXX`) удалён без замены: сервер на Windows останется
без сертификата.

**Фикс:** перенести присваивания внутрь ifdef и добавить WIN32-вариант
(`GetTempPath` + счётчик или `_mktemp_s`).

### BUG-13 (остаток): linux-клиент проверяет результат `connect()` как bool

**Файл:** `HandlerSslSocket.cpp`, `process()` (linux), POLLOUT-ветка.
**Статус: OPEN.**

```cpp
if (!connect())
{
    doError(SocketUtils::lastError());
    return false;
}
```

`SSL_connect` при провале возвращает `0` или `-1` — оба варианта дают
`!connect() == true` (повезло), но источник ошибки неверен: нужен
`SSL_get_error(_sslInstance, ret)`, а не `SocketUtils::lastError()`.
Референс — Windows-ветка `openTcpClientSocket` (`if (1 != lRet)` +
`SSL_get_error`). Удобно исправить одним коммитом с BUG-15.

### BUG-8: провал `SSL_CTX_new` остаётся «немым»

**Файл:** `HandlerSslSocket.cpp`, `sslInitialize()`. **Статус: OPEN.**

`if (!_sslContext) break;` не предотвращает `_sslInstance = SSL_new(_sslContext);`
после switch. В современных OpenSSL `SSL_new(NULL)` не падает, а возвращает
`NULL` (`ERR_R_PASSED_NULL_PARAMETER`), но сокет остаётся с `_sslInstance == nullptr`
без `doError` — тихо неработающий объект. Аналогично: в серверной ветке при
провале записи/загрузки сертификатов `break` приводит к созданию instance от
неподготовленного контекста. Отчасти смягчено guard'ом в `process()`
(`if (!_sslInstance) { doError... }`), но провал создания контекста стоит
обрабатывать сразу.

**Фикс:** после switch — `if (!_sslContext) { doError(...); return; }` перед `SSL_new`.

### BUG-5: `sectionParameterDelete()` и `sectionDelete()` отравляют `_sectionHash`

**Файл:** `threader.std/.../IniFile.cpp`. **Статус: OPEN.**

Методы читают `_sectionHash[sectionName]` через `operator[]` до проверки
существования — для несуществующего имени вставляется нулевая запись.
После этого `count(name) == 1`, и `sectionParameterCountByName(name)`
разыменовывает нулевой указатель — сегфолт.

**Репро:** `DISABLED_IniFileHash.DeleteUnknownSectionPoisonsLookup`,
`DISABLED_IniFileHash.DeleteWithEmptySectionNamePoisonsEmptyKey`.
**Фикс:** заменить `operator[]` на `find()` в обоих методах.

### BUG-4: regex в `parseContent()` (IniFile)

**Статус: OPEN.** Подробности — в предыдущих итерациях ревью; тесты-фиксации
в `tests/test_ini_file.cpp` (сюты `DISABLED_IniFile*`).

---

## NOTE (замечания, не блокирующие)

- **N1.** `static bool initialized` в `sslInitialize()` читается вне
  `call_once` — формально гонка/UB на неатомарной переменной, если первые
  два сокета создаются из разных потоков. `call_once` сам делает всё нужное —
  флаг можно просто удалить.
- **N2.** Результат `SSL_CTX_load_verify_locations` не проверяется (обе
  платформы). При битом/несуществующем CA диагностика откладывается до
  провала `SSL_connect` с невнятным сообщением.
- **N3.** Результат `mkstemp` не проверяется: при неудаче шаблон останется
  с литеральными `XXXXXX`, файл будет создан по другому пути и провалится
  уже `SSL_CTX_use_*_file`.
- **N4.** Путь `/tmp/` захардкожен вместо `FileUtils::pathtoTemp()` — на
  системах без `/tmp` или с переопределённым `TMPDIR` `mkstemp` упадёт.
- **N5.** Windows `openTcpClientSocket`: при провале TLS-хендшейка
  (`sslError != SSL_ERROR_WANT_READ`) возврат `INVALID_DESCRIPTOR` без
  закрытия `result` — утечка дескриптора.
- **N6.** Опечатки в именах: `useExtrenalCerts()`/`_useExtrenalCertificates`,
  `CERRTIFICATE`, параметр `primaryKeyFilename` (ключ, а не Primary key —
  речь о приватном ключе). Исправлять с переименованием API — осознанно.
- **N7.** `setInstanceDescriptor()`: недостижимый `return false;` после пар
  `#ifdef` (на linux — warning компилятора).
- **N8.** `<mutex>` подключён в хедере — после удаления флага `initialized`
  (N1) используется только `std::call_once` в .cpp.
- **N9.** Цена редизайна: каждый серверный сокет заново парсит встроенный
  2048-битный сертификат + 2 вызова `FileUtils::fileDelete` на конструктор.
  При росте нагрузки вернётся вопрос шаринга контекста (осознанно, с
  `std::call_once`/мьютексом).
- **N10.** Дублирующие присваивания в теле конструктора (члены уже
  инициализированы в списке инициализации).
- **N11.** Пост-хендшейк проверка `SSL_get_verify_result` в обеих ветках —
  по сути мёртвый код при `SSL_VERIFY_PEER`: успешный хендшейк уже
  гарантирует валидность. Не удалять (полезна при откате флага), но помнить.

---

## VERIFY (проверить на полной версии кода)

- Обработка `POLLHUP`/`POLLRDHUP` в `process()` (linux) при обрыве клиента
  в незавершённом хендшейке: ветка закрытия должна срабатывать до/вместо
  бесконечного `WANT_READ` от `SSL_accept` по закрытому дескриптору.
- Кто повторно вызывает `connect()` на Windows при `WANT_READ` в
  `openTcpClientSocket`: `processAccept()` обрабатывает только сервер,
  путь продолжения клиентского хендшейка не виден в видимой части кода.

---

## Состояние тестов

| Набор | Статус |
|---|---|
| `test_ssl_socket.cpp` (сюты `SslSocket.*`) | активные; `EachInstanceHasOwnContext`, `ExternalCertsFlagIsPerInstance`, `ReadWriteBeforeConnectionReturnZero`, `CipherInfoBeforeHandshakeIsNone`, `ClientSocketWithInvalidDescriptorIsDisconnected`, `ServerSocketWithValidDescriptorIsConnecting` — обязаны проходить |
| `test_ssl_handshake.cpp` (`SslHandshake.FullHandshakeAndEchoOverSocketPair`) | активный; хендшейк + эхо + CN `systemt.ru` + чистый teardown |
| `DISABLED_test_ssl_refcount.cpp` | удалён (BUG-1/2/3 закрыты редизайном) |
| `DISABLED_IniFileHash.*`, `DISABLED_IniFile*` | спят до фикса BUG-5/BUG-4 |
| `IniFile.CommentsUnknownSectionDoesNotPolluteHash` | активный, регрессионная защита |

**Прогон:**

```bash
cmake --build build -j$(nproc)
ctest --test-dir build -R ssl-tests -V
ctest --test-dir build -R std-tests --output-on-failure
./bin/debug/threader-ssl-tests --gtest_also_run_disabled_tests
```

**Планируемые тесты:** `test_ssl_accept.cpp` (эмуляция poller'а: клиент
шлёт ClientHello через `SSL_connect`, сервер проводит хендшейк через
`process()` по POLLIN; кейсы «клиент молчит» и «клиент рвёт соединение»);
`test_ssl_verify.cpp` (отказ верификации самоподписанного сертификата при
`verifyCertificate = true` с чужим CA; позитивный кейс со встроенным
сертификатом как CA-файлом).

**Следующий шаг:** закрыть BUG-15 + остаток BUG-13 одним коммитом (соседние
участки `HandlerSslSocket.cpp`), затем BUG-12 (Windows-сборка), затем
BUG-8 и BUG-5.
