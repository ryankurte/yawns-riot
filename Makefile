# OWNS Riot Makefile

APPLICATION = owns-riot
BOARD ?= native
RIOTBASE ?= $(CURDIR)/../riot-os

CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

USEMODULE += saul_default


USEMODULE += netif
USEMODULE += netdev_ieee802154


USEMODULE += gnrc
USEMODULE += gnrc_netdev
USEMODULE += gnrc_netif

USEMODULE += gnrc_rpl
USEMODULE += gnrc_sixlowpan

USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_udp

USEMODULE += auto_init_gnrc_rpl
USEMODULE += auto_init_gnrc_netif
#USEMODULE += auto_init_gnrc_ipv6_netif


USEMODULE += gnrc_pktdump
USEMODULE += gnrc_icmpv6_echo

USEMODULE += gcoap

# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += ps

USEMODULE += netstats_l2
USEMODULE += netstats_ipv6
USEMODULE += netstats_rpl

FEATURES_OPTIONAL += config

QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
CFLAGS += -DIEEE802154_DEFAULT_SUBGHZ_CHANNEL=5
CFLAGS += -DIEEE802154_DEFAULT_CHANNEL=26
CFLAGS += -g

LINKFLAGS += -lowns

run: ./bin/${BOARD}/${APPLICATION}.elf
	./$<
