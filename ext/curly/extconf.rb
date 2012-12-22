require 'mkmf'


find_library('curl', 'curl_easy_init',
  '/opt/local/lib',
  '/usr/local/lib',
  '/usr/lib'
)

create_makefile('curly/curly')

# This hack allows flymake-mode to run syntax checking.
File.open(File.join(File.dirname(__FILE__), "Makefile"), "a") do |f|
  f.write("check-syntax:\n")
  f.write("\t$(CC) $(CFLAGS) $(INCFLAGS) -Wall -fsyntax-only $(CHK_SOURCES)\n")
end
