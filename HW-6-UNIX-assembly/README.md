Для конфигарации использовал `./configure` со следующими флагами:
`--without-ssl --enable-http --disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp --disable-proxy --disable-dict --enable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smb --disable-smtp --disable-gopher --disable-mqtt --disable-sspi`

![[cmd_configure.png]]
результат выполнения `./configure`
![[configure_res.png]]

проверяем локальный `curl`:
![[version_curl.png]]