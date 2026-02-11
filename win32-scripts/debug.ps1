$env:PATH = "C:\mingw64\bin;$env:PATH"
make -f makeWin32.mk
gdb .\CoBaLT