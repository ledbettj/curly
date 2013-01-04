require 'sinatra/base'
require 'json'

TEST_URL = "http://localhost:4567"

class SpecServer < Sinatra::Base
  def self.start
    # Oh my god webrick/sinatra shut up I'm trying to run specs here.
    $stderr.reopen('/dev/null', 'w')

    Thread.new { SpecServer.run! }
    sleep 2
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

    response.headers['X-Sample-Header']  = '123'
    response.headers['X-Another-Header'] = '456'

    JSON.dump(r)
  end

  get '/cookie-test' do
    response.set_cookie('cookie-1', :value => 'abc')
    response.set_cookie('cookie-2', :value => 'def')
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
