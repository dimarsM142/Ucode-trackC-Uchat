LIBBIN := libmx.a sqlite.a
CLIBIN := uchat
SRVBIN := uchat_server
LIBDIR := libmx
SQLDIR := sqlite3
CLIDIR := uclient
SRVDIR := userver
CERT := ssl_certificate.pem

all: children keygen
	mv $(CLIDIR)/$(CLIBIN) .
	mv $(SRVDIR)/$(SRVBIN) .

children:
	$(MAKE) -C $(SQLDIR)
	$(MAKE) -C $(LIBDIR)
	$(MAKE) -C $(CLIDIR)
	$(MAKE) -C $(SRVDIR)

keygen: 
	openssl req -x509 -nodes -days 365 -newkey ed25519 -subj "/C=US/ST=Denial/L=Springfield/O=Dis/CN=www.example.com" -keyout $(CERT) -out $(CERT)

uninstall: clean
	$(MAKE) -C $(SQLDIR) uninstall
	$(MAKE) -C $(CLIDIR) uninstall
	$(MAKE) -C $(SRVDIR) uninstall
	$(MAKE) -C $(LIBDIR) uninstall
	rm -f $(CLIBIN) $(SRVBIN) $()

clean:
	$(MAKE) -C $(SQLDIR) clean
	$(MAKE) -C $(CLIDIR) clean
	$(MAKE) -C $(SRVDIR) clean
	$(MAKE) -C $(LIBDIR) clean

reinstall: uninstall all
