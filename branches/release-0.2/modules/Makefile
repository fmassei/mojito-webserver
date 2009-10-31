include ../Makefile.flags

all:
	@(cd mod_stat && make all)
	@(cd mod_cacheshm && make all)
	@(cd mod_gzip && make all)
	@(cd mod_deflate && make all)
	@(cd mod_identity && make all)
	@(cd mod_cgi && make all)
	@(cd mod_fastcgi && make all)
	-$(RM) *.oh *.ot

clean:
	@(cd mod_stat && make clean)
	@(cd mod_cacheshm && make clean)
	@(cd mod_gzip && make clean)
	@(cd mod_deflate && make clean)
	@(cd mod_identity && make clean)
	@(cd mod_cgi && make clean)
	@(cd mod_fastcgi && make clean)
	-$(RM) *.oh *.ot
