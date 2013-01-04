#!/usr/bin/env rake
require "bundler/gem_tasks"
require "rake/extensiontask"
require "rspec/core/rake_task"
require "rdoc/task"

Rake::ExtensionTask.new("curly") do |ext|
  ext.lib_dir = 'lib/curly'
end

RSpec::Core::RakeTask.new("spec")

Rake::RDocTask.new do |doc|
  doc.rdoc_dir = "rdoc"
  doc.title    = "Curly"

  doc.rdoc_files.include("lib/")
  doc.rdoc_files.include("ext/")
end

task "console" do
  exec 'pry -r curly'
end
