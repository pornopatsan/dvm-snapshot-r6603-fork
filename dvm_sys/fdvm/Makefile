
SHELL = /bin/sh
INSTALL = /bin/cp

SUBDIR	= Sage parser fdvm

install: 
	     @for i in ${SUBDIR}; do (cd $$i; \
	     echo "   ***  $$i DIRECTORY   ***";\
             $(MAKE) "MAKE=$(MAKE)" install); done

clean:      
	     @for i in ${SUBDIR}; do (cd $$i; \
	     echo "   ***  $$i DIRECTORY   ***";\
             $(MAKE) "MAKE=$(MAKE)" clean); done


