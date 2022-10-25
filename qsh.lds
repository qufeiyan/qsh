SECTIONS {
    .qsh.cmd.tab : {
        . = ALIGN(4);
        __QSH_CMD_TAB_START = .;
        KEEP(*(.qsh.cmd.tab));
        . = ALIGN(4);
        __QSH_CMD_TAB_END = .;
    }

    .qsh.var.tab : {
        . = ALIGN(4);
        __QSH_VAR_TAB_START = .;
        KEEP(*(.qsh.var.tab));
        . = ALIGN(4);
        __QSH_SHELL_VAR_END = .;
    }

    PROVIDE(__QSH_SHELL_CMD_START = __QSH_CMD_TAB_START);
    PROVIDE(__QSH_SHELL_CMD_END = __QSH_CMD_TAB_END);
    PROVIDE(__QSH_SHELL_VAR_START = __QSH_VAR_TAB_START);
    PROVIDE(__QSH_SHELL_VAR_END = __QSH_VAR_TAB_END);
}

INSERT AFTER .text