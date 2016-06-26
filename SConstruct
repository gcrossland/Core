import sys
try:
  import sconsutils
except ImportError:
  raise ImportError("Failed to import sconsutils (is buildtools on PYTHONPATH?)"), None, sys.exc_traceback

env = sconsutils.getEnv()
for n in ("integers", "bitwork"):
  env.Command(n + ".cpp", "make_" + n + ".py", [[sys.executable, "make_" + n + ".py"]])
  # Partially work around lost dependencies (see
  # http://scons.tigris.org/issues/show_bug.cgi?id=2908).
  env.Depends(env['oDir'] + "/" + n + ".cpp", n + ".cpp")
env.InVariantDir(env['oDir'], ".", lambda env: env.LibAndApp('core', 0, -1, None))
