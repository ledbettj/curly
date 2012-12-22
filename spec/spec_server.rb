require 'sinatra/base'
require 'json'

class SpecServer < Sinatra::Base
  def self.start
    @pid = fork {
      $stdout.reopen("/dev/null")
      $stderr.reopen("/dev/null")
      SpecServer.run!
    }
    sleep 2
  end

  def self.stop
    Process.kill("SIGTERM", @pid)
  end

  get '/status-test' do
    status 418
  end

  get '/body-test' do
    JSON.dump({
        :value => 1234
      })
  end
end
