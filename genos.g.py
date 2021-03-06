from licant.modules import module
import traceback

#licant.include("periph")
licant.include("igris")
licant.include("nos")

licant.execute_recursive("src", ".g.py")
licant.execute_recursive("arch", ".g.py")
licant.execute_recursive("board", ".g.py")

module ("genos.include", 
	include_paths = [".", "src"],
	mdepends = ["igris.include"] )

module ("genos.irqtable", sources = [ "src/hal/irqtable.cpp" ] )
module ("genos.malloc", "lin", 
	sources=["src/mem/lin_malloc.cpp", "src/mem/lin_realloc.cpp"], 
	default=True
)

module("genos.addons.adafruit_motor_shield", sources = [
	"src/addons/Adafruit_MotorShield/Adafruit_MS_PWMServoDriver.cpp",
	"src/addons/Adafruit_MotorShield/Adafruit_MotorShield.cpp",
])

module("genos.addons.OneWire", sources = [
	"src/addons/OneWire/OneWire.cpp"
])

module("genos.sched", "stub",
	sources = ["src/sched/waitstub.c"],
)

module("genos.schedee_manager",
	sources = [
		"src/genos/sched.cpp",
		"src/genos/schedee.cpp",
	],
)

module("genos.ktimer_manager",
	sources = [
		"src/genos/ktimer.cpp",
	],
)

module("genos.sched", "impl",
	sources = [
		"src/genos/sched.cpp",
		"src/genos/schedee.cpp",
		"src/genos/apitime.cpp",
		"src/genos/schedee/autom.cpp",
		"src/genos/schedee/coop.cpp",

		"src/sched/posix/fcntl_mvfs.cpp",
		"src/sched/posix/unistd_mvfs.cpp",

		"src/genos/wait.cpp",
		"src/genos/ktimer.cpp",

		"src/genos/fops.cpp",
		
		"src/genos/resource.cpp",
		"src/genos/resmngr.cpp",
		"src/genos/nav.cpp",
		"src/genos/dev.cpp",
		
		"src/genos/errno.cpp"
	],
	default=True,
)

module(
	"genos.makeproc", 
	sources=["src/genos/schedee/oneshoot_makeproc.cpp"]
)

module("genos.sched", "test",
	sources = [
		"src/genos/sched.cpp",
		"src/genos/schedee.cpp",
		"src/genos/apitime.cpp",
		"src/genos/schedee/autom.cpp",

		"src/sched/posix/fcntl_mvfs.cpp",
		"src/sched/posix/unistd_mvfs.cpp",

		"src/genos/ktimer.cpp",

		#"src/genos/fops.cpp",
		
		"src/genos/resource.cpp",
		"src/genos/resmngr.cpp",
		"src/genos/nav.cpp",
		"src/genos/dev.cpp",
		
		"src/genos/errno.cpp"
	]
)

module("genos.mvfs", 
	sources = [
		"src/mvfs/mvfs.c",
		"src/mvfs/fstype.c",
		"src/mvfs/super.c",
		"src/mvfs/node.c",
		"src/mvfs/pathops.c",
		"src/mvfs/lookup.c",
		"src/mvfs/file.c",
		
		"src/drivers/cdev/cdev.c",
		"src/drivers/cdev/virtual/null.c",
		"src/drivers/cdev/virtual/zero.c",
		"src/drivers/cdev/virtual/debug.c",

		"src/mvfs/fs/joke/joke.c"
	],

	defines = [
		("MVFS_INCLUDED")
	]
)

module("genos.mvfs.global", 
	sources = [ "src/mvfs/variant/global_root.c", "src/mvfs/variant/global_pwd.c" ]
)

module("genos.mvfs.schedee_support", 
	sources = [ "src/mvfs/variant/global_root.c", "src/mvfs/variant/schedee_support.c" ]
) 

module ("genos.systime", srcdir="src", sources = [ "systime/systime.c" ] )

module ("genos.errno", srcdir="src", sources = [ "errno.c" ])
module ("genos.cpudelay", srcdir="src", sources = [ "hal/cpudelay.c" ])

module ("genos.utility.numcmd", sources = ["src/utility/numcmd.cpp"], mdepends=["igris.protocols.numcmd"])
module ("genos.utility.contty", sources = ["src/utility/contty.cpp"])
module ("genos.utility.contty2", sources = ["src/utility/contty2.cpp"])
module ("genos.utility.contty3", sources = ["src/utility/contty3.c"])
module ("genos.utility.contty4", sources = ["src/utility/contty4.cpp"])
module ("genos.utility.mshell", sources = ["src/utility/mshell.c"])

module("genos.diag", "stub", sources = ["src/diag/diag_stub.c"])
module("genos.diag", "impl", sources = ["src/diag/diag_impl.c"], default=True)

licant.module("igris.dprint", "diag", 
	srcdir="src/diag",
	sources = ["dprint_diag.c"],
	mdepends = ["igris.dprint.common", "genos.diag"],
)

licant.module_default_implementation("igris.dprint", "diag")
licant.module_default_implementation("igris.syslock", "genos.atomic")
licant.module_default_implementation("nos.current_ostream", "nullptr")

module("genos",
	mdepends = 
	[
		"igris.include",
		"igris.util",
		"igris.bug",
		"igris.dprint",
		"igris.syslock",
		"igris.cxx_support",
		"igris.ctrobj.common",

		"genos.include",
		"genos.sched",
		"genos.irqtable",
		"genos.diag",
		"genos.systime",
		"genos.syscmd",
		#"genos.regmap",

		"genos.makeproc"
	]
)

module("genos.syscmd",
	sources=[
		"src/genos/syscmd.cpp"
	])

module("genos.regmap",
	sources=[
		"src/drivers/regmap.cpp"
	])

def genos_firmware(sources=[], mdepends=[], **kwargs):
	import shutil
	import licant.modules

	if os.path.exists("__configure__.py"):
		try:
			import __configure__
		except:
			print("Error in configuration file.")		

	dir_path = licant.modules.mlibrary.get("genos").opts["__dir__"]

	@licant.routine
	def configure(board):
		print("load configuration script for {}".format(board))

		confpath = os.path.join(dir_path, "conf", "conf-" + board + ".py")


		shutil.copy(
			confpath,
			"__configure__.py"
		)

	if not os.path.exists("__configure__.py"):
		@licant.routine
		def firmware(*args, **kwargs):
			print("Enougth of configuration file")
	else:
		try:
			licant.include("igris")

			all_modules=[]
			all_modules.extend(mdepends)
			all_modules.extend(__configure__.mdepends)
			all_modules.extend(["genos"])
			
			licant.cxx_application("firmware",
				toolchain=__configure__.toolchain,
				sources=sources,
				mdepends=all_modules,
				**kwargs
			)

			@licant.routine(deps=["firmware"])
			def install(*args):
				__configure__.install("firmware", *args)

			@licant.routine
			def terminal(*args):
				__configure__.terminal(*args)

		except Exception as e:
			print(e)
			traceback.print_exc()
			print("Error in firmware module. Configuration problem?")

	licant.ex("firmware")

licant.global_function(genos_firmware)



