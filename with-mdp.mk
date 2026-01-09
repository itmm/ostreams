INC_DIR ?= ..
include $(INC_DIR)/base.mk

$(GENERATED): .mdp_run

.mdp_run: $(wildcard *.md)
	chmod u+w $(GENERATED)
	[ -x "$$(command -v mdp)" ] || echo "mdp not installed" 1>&2
	[ -x "$$(command -v mdp)" ] && mdp README.md
	chmod a-w $(GENERATED)
	date >$@
