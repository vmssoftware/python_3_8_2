import sys
import dtr
import vms.ssdef as SS

sts, dab = dtr.init(100, 0)

if sts != SS.SS__NORMAL:
	print("Initialization failed")
	sys.exit(0)

sts, cond, state = dtr.command(dab, "set dictionary cdd$top.dtr$lib.demo.rdb;")

if sts != SS.SS__NORMAL:
	print("Command failed")
	sys.exit(0)

if state == dtr._K_STL_MSG:
	print(dtr.msg_buf(dab))
	dtr.cont(dab)

sts, cond, state = dtr.command(dab, "ready jobs shared read;")

if sts != SS.SS__NORMAL:
	print("Command failed")
	sys.exit(0)

while state == dtr._K_STL_MSG:
	print(dtr.msg_buf(dab))
	sts, cond, state = dtr.cont(dab)

sts, cond, state = dtr.command(dab, "print jobs;")

if sts != SS.SS__NORMAL:
	print("Command failed")
	sys.exit(0)

sts = dtr.skip(dab, 4)

fmt = "%4c   %1c   %20c %11c  %11c"

while dtr.state(dab) == dtr._K_STL_LINE:
	row = dtr.row(dab, fmt)
	print(row)

sts = dtr.finish(dab)

if sts != SS.SS__NORMAL:
	print("Problems tidying up")
	sys.exit(0)

