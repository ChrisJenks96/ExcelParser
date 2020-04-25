# Excel Parser

This Excel Parser is just a fun exercise to parse the XLSX file format

## Dependancies

[Minizip](https://github.com/nmoinvaz/minizip) - this is used to decompress the ZIP files and extract the XML files

## Limitations

The maximum length of a cell name can only be 16 bytes long - to avoid using new/delete during runtime
