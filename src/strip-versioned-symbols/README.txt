ELF patching tool that strips versioned symbol information from dynamically
linked binaries.

Use this to build a binary with a new toolchain that can run on older systems.

`objcopy --remove-section .gnu.version --remove-section .gnu.version_r` should
be used to remove symbol versioning sections from your binary.
`strip-versioned-symbols` removes the remaining references to these sections
from the .dynamic section by removing all DT_VERSYM, DT_VERNEED, and
DT_VERNEEDNUM entries.

Removing these sections without removing the .dynamic entries that point to
them will make ld.so complain with "unsupported version 0 of Verneed record" or
similar; ld.so will still follow the file offset listed in DT_VERNEED even if
there are no sections of type `SHT_GNU_verneed`.

Some symbol versioning detritus (e.g. strings that look like "GLIBC_2.2.5")
will remain in your binary's .dynstr section.

Link your binaries with `-Wl,--hash-style=both` for compatibility with really
old glibc (pre-871b91589bf4f6dfe19d5987b0a05bd7cf936ecc, around 2006-06-10)
versions that don't support DT_GNU_HASH-only binaries.

ELF symbol versioning is a misfeature. Existing workarounds for building
executables using new toolchains while maintaining compatibility with older
systems are ludicrous and labor-intensive. As far as I can tell, neither strip
nor objcopy allow you to remove DT_VERSYM, DT_VERNEED, or DT_VERNEEDNUM. And
neither ld nor lld let you opt out of using symbol versioning.

Thanks to Mark Laws (https://github.com/drvink/) for his knowledge of ELF
esoterica and for random related conversations.

Also see:
- https://refspecs.linuxfoundation.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/symversion.html
