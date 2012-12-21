#!/usr/bin/env rake
require "bundler/gem_tasks"

task :native do
  `cd ./ext/curly && make && cp *.bundle ../../lib/curly/`
end
