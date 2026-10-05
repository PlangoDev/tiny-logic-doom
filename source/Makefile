# DOOM (doomgeneric, the shareware WAD inside the image) for TINY-32: RV32IM, no operating system.
#   make              -> $(B)/doom.bin (load at address 0)
LLVM ?= /opt/homebrew/opt/llvm/bin
LLD ?= /opt/homebrew/opt/lld/bin
CC = $(LLVM)/clang
XLEN ?= 32
ARCH = --target=riscv$(XLEN)-unknown-elf -march=rv$(XLEN)im -mabi=$(if $(filter 64,$(XLEN)),lp64,ilp32)
PAL ?= 0
B = build$(if $(filter 64,$(XLEN)),64,)$(if $(filter 1,$(PAL)),-pal,)
CFLAGS = $(ARCH) -O3 -ffreestanding -fno-builtin-printf -nostdlibinc -Iport/include -Idoomgeneric/doomgeneric \
         -DCMAP256 -DDOOMGENERIC_RESX=320 -DDOOMGENERIC_RESY=200 -fno-common -ffunction-sections -fdata-sections \
         -Wno-everything $(if $(filter 1,$(PAL)),-DTINY32_PALETTE,)
DOOM = dummy am_map doomdef doomstat dstrings d_event d_items d_iwad d_loop d_main d_mode d_net f_finale f_wipe g_game \
       hu_lib hu_stuff info i_cdmus i_endoom i_joystick i_scale i_sound i_system i_timer memio m_argv m_bbox m_cheat \
       m_config m_controls m_fixed m_menu m_misc m_random p_ceilng p_doors p_enemy p_floor p_inter p_lights p_map \
       p_maputl p_mobj p_plats p_pspr p_saveg p_setup p_sight p_spec p_switch p_telept p_tick p_user r_bsp r_data \
       r_main r_plane r_segs r_sky r_things sha1 sounds statdump st_lib st_stuff s_sound tables v_video \
       wi_stuff w_checksum w_file w_main w_wad z_zone w_file_stdc i_input doomgeneric
RT = addsf3 divsf3 mulsf3 extendsfdf2 fixsfsi floatsisf comparedf2 fp_mode
OBJS = $(addprefix $(B)/,$(addsuffix .o,$(DOOM))) $(B)/doomgeneric_tiny32.o $(B)/i_video_tiny32.o $(B)/r_draw_tiny32.o $(B)/libc.o $(B)/builtins.o $(B)/crt0.o \
       $(addprefix $(B)/rt_,$(addsuffix .o,$(RT)))

all: $(B)/doom.bin

$(B)/%.o: doomgeneric/doomgeneric/%.c | $(B)
	$(CC) $(CFLAGS) -c $< -o $@
$(B)/%.o: port/%.c | $(B)
	$(CC) $(CFLAGS) -c $< -o $@
$(B)/rt_%.o: compiler-rt/%.c | $(B)
	$(CC) $(ARCH) -O2 -ffreestanding -nostdlibinc -Iport/include -Icompiler-rt -Wno-everything -c $< -o $@
$(B)/crt0.o: port/crt0.S doom1.wad | $(B)
	$(CC) $(ARCH) -c port/crt0.S -o $@
$(B):
	mkdir -p $(B)

$(B)/doom.elf: $(OBJS) port/link.ld
	$(LLD)/ld.lld -T port/link.ld --gc-sections -o $@ $(OBJS)
$(B)/doom.bin: $(B)/doom.elf
	$(LLVM)/llvm-objcopy -O binary $< $@
	$(LLVM)/llvm-size $<

clean:
	rm -rf build build-pal build64 build64-pal
