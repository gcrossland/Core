import os, imp
sconsutils = imp.load_source('sconsutils', os.path.join(os.pardir, os.pardir, "build", "sconsutils.py"))
import sys

env = sconsutils.getEnv()
for n in ("integers", "bitwork"):
  env.Command(n + ".cpp", "make_" + n + ".py", [[sys.executable, "make_" + n + ".py"]])
  # Partially work around lost dependencies (see
  # http://scons.tigris.org/issues/show_bug.cgi?id=2908).
  env.Depends(env['oDir'] + "/" + n + ".cpp", n + ".cpp")
env.InVariantDir(env['oDir'], ".", lambda env: env.LibAndApp('core', 0, -1, None))
