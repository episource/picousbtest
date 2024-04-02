Import("env")
env['PROJECT_SRC_DIR'] = env['PROJECT_DIR'] + "\\src\\" + env["PIOENV"]
env['PROJECT_INCLUDE_DIR'] = env['PROJECT_DIR'] + "\\include\\" + env["PIOENV"]