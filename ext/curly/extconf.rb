require 'mkmf'


find_library('curl', 'curl_easy_init',
  '/opt/local/lib',
  '/usr/local/lib',
  '/usr/lib'
)

dir_config('curly')
create_makefile('curly')
