# -*- encoding: utf-8 -*-
require File.expand_path('../lib/curly/version', __FILE__)

Gem::Specification.new do |gem|
  gem.authors       = ["John Ledbetter"]
  gem.email         = ["john.ledbetter@oracle.com"]
  gem.description   = %q{A gem description}
  gem.summary       = %q{A gem summary}
  gem.homepage      = ""

  gem.files         = `git ls-files`.split($\)
  gem.extensions    = ['ext/curly/extconf.rb']
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.name          = "curly"
  gem.require_paths = ["lib"]
  gem.version       = Curly::VERSION

  gem.add_development_dependency 'pry'
  gem.add_development_dependency 'rake'
  gem.add_development_dependency 'rake-compiler'
  gem.add_development_dependency 'rspec'
  gem.add_development_dependency 'sinatra'
  gem.add_development_dependency 'activesupport'
end
