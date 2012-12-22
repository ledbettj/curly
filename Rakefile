#!/usr/bin/env rake
require "bundler/gem_tasks"

task :native do
  `cd ./ext/curly && ruby ./extconf.rb && make && cp *.bundle *.so *.dll ../../lib/curly/`
end
