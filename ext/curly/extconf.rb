require 'mkmf'


find_library('curl', 'curl_easy_init',
  '/opt/local/lib',
  '/usr/local/lib',
  '/usr/lib'
)

create_makefile('curly')
