COPY    START   1000
FIRST   STL     RETADR
CLOOP   JSUB    RDREC
        LDA     LENGTH
        COMP    ZERO
        JEQ     ENDFIL
        JSUB    WRREC
        J       CLOOP
ENDFIL  LDA     EOF
        STA     BUFFER
        LDA     THREE
        STA     LENGTH
        JSUB    WRREC
        LDL     RETADR
        RSUB
RETADR  RESW    1
LENGTH  RESW    1
ZERO    WORD    0
THREE   WORD    3
EOF     BYTE    C'EOF'
BUFFER  RESB    4096
        END     FIRST
