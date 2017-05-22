call build debug
rem if errorlevel 1 goto :eof
pushd .runtime
Hackaton2017_debug %*
popd
