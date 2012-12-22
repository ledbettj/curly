require 'sinatra/base'

class SpecServer < Sinatra::Base
  get '/' do
    "hello world"
  end

  def self.start
    @pid = fork {
      SpecServer.run!
    }
    sleep 2
  end

  def self.stop
    Process.kill("SIGTERM", @pid)
  end


end
