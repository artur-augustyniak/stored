AUTOMAKE_OPTIONS = foreign
SUBDIRS = src doc examples man scripts systemd

DISTCHECK_CONFIGURE_FLAGS = \
        --with-systemdsystemunitdir=$$dc_install_base/$(systemdsystemunitdir)
