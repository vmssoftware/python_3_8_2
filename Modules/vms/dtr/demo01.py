# Before running this example "copy dtr$library:yacht.dat []"
#
import sys
import rec
import dtr
import vms.ssdef as SS

r = rec.new()
rec.addstr(r, None, 10, 0)
rec.addstr(r, None, 10, 10)
rec.addstr(r, None, 6, 20)
rec.addstr(r, None, 3, 26)
rec.addstr(r, None, 5, 29)
rec.addstr(r, None, 2, 34)
rec.addstr(r, None, 5, 36)

sts, dab = dtr.init(100, 0)

if sts != SS.SS__NORMAL:
	print("Initialization failed")
	sys.exit(0)

sts, cond, state = dtr.command(dab, "set dictionary cdd$top.dtr$lib.demo;")

if sts != SS.SS__NORMAL:
	print("Command failed")
	sys.exit(0)

if state == dtr._K_STL_MSG:
	print(dtr.msg_buf(dab))
	dtr.cont(dab)

sts, cond, state = dtr.command(dab, "declare port yport using yacht;")

if sts != SS.SS__NORMAL:
	print("Command failed")
	sys.exit(0)

while state == dtr._K_STL_MSG:
	print(dtr.msg_buf(dab))
	sts, cond, state = dtr.cont(dab)

sts, cond, state = dtr.command(dab, "ready yachts; ready yport write;")

if sts != SS.SS__NORMAL:
	print("Command failed")
	sys.exit(0)

while state == dtr._K_STL_MSG:
	print(dtr.msg_buf(dab))
	sts, cond, state = dtr.cont(dab)

sts, cond, state = dtr.command(dab, "yport = yachts with loa > 30;")

if sts != SS.SS__NORMAL:
	print("Command failed")
	sys.exit(0)

while state == dtr._K_STL_MSG:
	print(dtr.msg_buf(dab))
	sts, cond, state = dtr.cont(dab)

while state == dtr._K_STL_PGET:
	sts = dtr.get_port(dab, r)

	if sts != SS.SS__NORMAL:
		print("Unable to retrieve data")
		sys.exit(0)

	print("%-10s\t%-10s\t%-6s\t%-3s\t%-5s\t%-2s\t%-5s" % (rec.getstr(r, 0), rec.getstr(r, 1), rec.getstr(r, 2), rec.getstr(r, 3), rec.getstr(r, 4), rec.getstr(r, 5), rec.getstr(r, 6)))
	state = dtr.state(dab)

sts = dtr.finish(dab)

if sts != SS.SS__NORMAL:
	print("Problems tidying up")
	sys.exit(0)

rec.delete(r)
