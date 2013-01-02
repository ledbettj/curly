require 'sinatra/base'
require 'json'

TEST_URL = "http://localhost:4567"

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

  get '/params-test' do
    JSON.dump(params)
  end

  get '/headers-test' do
    r = request.env.select{|k, v| k =~ /^HTTP_/ }

    JSON.dump(r)
  end

  post '/post-test' do
    json = JSON.parse(request.body.read)

    JSON.dump(json)
  end

  put '/put-test' do
    json = JSON.parse(request.body.read)

    JSON.dump(json)
  end

  delete '/delete-test' do
    status 200
  end

  get '/timeout-test' do
    sleep 3
    "done"
  end
end
