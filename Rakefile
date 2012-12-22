#!/usr/bin/env rake
require "bundler/gem_tasks"
require "rake/extensiontask"
require "rspec/core/rake_task"

Rake::ExtensionTask.new("curly") do |ext|
  ext.lib_dir = 'lib/curly'
end

RSpec::Core::RakeTask.new("spec")
