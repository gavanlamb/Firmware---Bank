# bank
A mongoose os application to control a bank of relays.

Change the bank.relay_pins value to define the pins that are going to be used for relay control. The order of pins
defines the alias and the alias is used in the topic to turn on and off the relays.  
EG
```json
["bank.relay_pins", "s", "5,4,0,2", {"title": "Array of pins associated to each relay"}]
```
alias '1' = pin 5, alias '2' = pin 4 and so on.

The topic /device/id/on/1 should actuate relay 1(pin 5).


## Build
| Build   | Command                                                                           |
|:--------|:----------------------------------------------------------------------------------|
| Heaplog | mos build --build-var MGOS_ENABLE_HEAP_LOG:1 --build-var MGOS_ENABLE_CALL_TRACE:1 |
| Release | mos build --clean --local                                                         |

## Flash
```console
mos flash
```


## Debug with heap allocator
Collect logs by
```console
mos console | tee console.log | grep -av 'hl{'
```

Run the heaplog server  
```console
./heaplog_server  --document_root .. --binary={directory-to}\bank\build\objs\fw.elf --console_log {directory-to}\bank\console.log
```
