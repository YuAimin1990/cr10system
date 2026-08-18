# make sure MAKECMDGOALS only contains make config-.... and nothing else

# if MAKECMDGOALS contains config-%
ifneq (,$(filter config-%,$(MAKECMDGOALS)))
  # and if MAKECMDGOALS contains anything but one config-%
    ifeq ($(word 1,$(MAKECMDGOALS)),$(MAKECMDGOALS))
        # all good
    else # not good
        $(error You can only specify one config-XXX target and it must be the only target)
    endif #
endif #

ifneq (,$(filter config-%,$(MAKECMDGOALS)))
    ifneq (,$(realpath $(PRJDIR)/.product_profile))
        $(Build is already configured for $(patsubst configs/%/board.config,%,$(shell readlink $(PRJDIR)/.product_profile)))
    endif
endif

config-%::
	@if [ -f configs/$(subst -,/,$(patsubst config-%,%,$@))/board.config ] ; then \
		echo Configuring Build using configs/$(subst -,/,$(patsubst config-%,%,$@))/board.config; \
		if [ -f .product_profile ]; then \
			tmp=$(shell diff .product_profile configs/$(subst -,/,$(patsubst config-%,%,$@))/board.config > /dev/null; echo $$?); \
			if [ $$tmp != 0 ]; then \
				echo -e '\e[31;1m" >>>>> "please make clobber before making new config...." >>>>> \033[0m'; \
				echo -e '\e[31;1m" >>>>> "exit ...." >>>>> \033[0m'; \
				exit 1; \
			fi; \
			rm -f .product_profile; \
		fi; \
		ln -sf configs/$(subst -,/,$(patsubst config-%,%,$@))/board.config .product_profile; \
		echo -e '\033[32;1m configuration written to .product_profile >>>>> \033[0m'; \
	fi

# vim: syntax=make
