# -*- encoding: utf-8 -*-
require File.expand_path('../lib/curly/version', __FILE__)


Gem::Specification.new do |gem|
  gem.authors       = ["John Ledbetter"]
  gem.email         = ["john@throttle.io"]
  gem.description   = %q{A thin libcurl wrapper for ruby.}
  gem.summary       = %q{A thin libcurl wrapper for ruby.}
  gem.homepage      = "http://github.com/ledbettj/curly"

  gem.files         = `git ls-files`.split($\)
  gem.extensions    = ['ext/curly/extconf.rb']
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.name          = "curly"
  gem.require_paths = ["lib"]
  gem.version       = Curly::VERSION

  ['pry', 'rake', 'rake-compiler', 'rspec', 'sinatra', 'activesupport'].each do |d|
    gem.add_development_dependency(d)
  end
end
