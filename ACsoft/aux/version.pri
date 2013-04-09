# ACRoot version
VERSION = 5.2.0
DEFINES += ACPASSTAG=\\\"$$VERSION\\\" ACROOTVERSION=52

# AMS Patch level
AMSPATCHLEVEL_DEFAULT = vdev

AMSPATCHLEVEL_ENV = $$(AMSPATCHLEVEL)
isEmpty(AMSPATCHLEVEL) {
  isEmpty(AMSPATCHLEVEL_ENV) {
    AMSPATCHLEVEL = $${AMSPATCHLEVEL_DEFAULT}
  } else {
    AMSPATCHLEVEL = $${AMSPATCHLEVEL_ENV}
  }
}