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

public __get_gdt, __set_gdt

.code

; void __get_gdt(descriptor_t *gdtr)
__get_gdt:
        sgdt fword ptr [rcx]
        ret

; void __set_gdt(descriptor_t *gdt);
__set_gdt:
        lgdt fword ptr [rcx]
        ret

end
