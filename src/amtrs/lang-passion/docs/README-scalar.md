# template arguments

|type|size|note|
|--|--|--|
|char|1||
|int8_t|1||
|uint8_t|1||
|short|2||
|int16_t|2||
|uint16_t|2||
|int32_t|4||
|uint32_t|4||
|long|8||
|int64_t|8||
|uint64_t|8||
|int|不明|VMに依存する。32bit環境なら32bit, 64bit環境なら64bitになります|
|intmax_t|不明|void*に一致する|

## psn::constring

内部的に扱われる静的な文字列。書き換えは不可能な値です。
