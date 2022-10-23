SECTIONS {
    .qsh.tab : {
        . = ALIGN(4);
        __QSH_TAB_START = .;
        KEEP(*(.qsh.tab));
        __QSH_TAB_END = .;
        . = ALIGN(4);
    }

    PROVIDE(__QSH_SHELL_START = __QSH_TAB_START);
    PROVIDE(__QSH_SHELL_END = __QSH_TAB_END);
}

INSERT AFTER .text