netpbm_cflags != pkg-config --cflags netpbm
netpbm_libs   != pkg-config --libs   netpbm
CFLAGS  += $(netpbm_cflags)
LDFLAGS += $(netpbm_libs)
