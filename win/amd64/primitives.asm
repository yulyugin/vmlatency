;
; Copyright (c) 2022 Evgenii Iuliugin
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program. If not, see <http://www.gnu.org/licenses/>.
;

public __get_gdt, __set_gdt, __sldt, __lar, __str
public __get_es, __get_cs, __get_ss, __get_ds, __get_fs, __get_gs, __get_ds

.code

; void __get_gdt(descriptor_t *gdtr);
__get_gdt:
        sgdt fword ptr [rcx]
        ret

; void __set_gdt(descriptor_t *gdt);
__set_gdt:
        lgdt fword ptr [rcx]
        ret

; u16 __sldt(void);
__sldt:
        sldt rax
        ret

; u32 __lar(u16 seg)
__lar:
        lar rax, rcx
        ret

; u16 __str(void);
__str:
        str rax
        ret

; u16 __get_es(void);
__get_es:
        mov rax, es
        ret

; u16 __get_cs(void);
__get_cs:
        mov rax, cs
        ret

; u16 __get_ss(void);
__get_ss:
        mov rax, ss
        ret

; u16 __get_ds(void);
__get_ds:
        mov rax, ds
        ret

; u16 __get_fs(void);
__get_fs:
        mov rax, fs
        ret

; u16 __get_gs(void);
__get_gs:
        mov rax, gs
        ret

end
