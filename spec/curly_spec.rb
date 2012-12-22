require 'spec_helper'
require 'spec_server'
require 'curly'
require 'pry'

TEST_URL = "http://localhost:4567"
describe "Curly" do

  before(:all) do
    SpecServer.start
  end

  after(:all) do
    SpecServer.stop
  end

  it "returns the appropriate status code" do
    resp = Curly::Request.get("#{TEST_URL}/status-test")

    resp.status.should eq(418)
  end

  it "returns the correct body" do
    resp = Curly::Request.get("#{TEST_URL}/body-test")

    resp.status.should eq(200)
    JSON.parse(resp.body).should eq({
        'value' => 1234
      })
  end

  it "handles params passed in the URL" do
    resp = Curly::Request.get("#{TEST_URL}/params-test?x=a&y=b&test=hello+world")
    JSON.parse(resp.body).should eq({
        'x' => 'a',
        'y' => 'b',
        'test' => 'hello world'
      })
  end

  it "handles setting headers" do
    resp = Curly::Request.get("#{TEST_URL}/headers-test",
      :headers => {
        'X-Example-Header' => 'five'
      }
    )

    r = JSON.parse(resp.body)
    r['HTTP_X_EXAMPLE_HEADER'].should eq('five')
  end

  it "handles posting data" do
    body = {
      'hello' => 'world',
      'hi' => 3
    }

    resp = Curly::Request.post("#{TEST_URL}/post-test",
      :headers => {
        'Content-Type' => 'application/json'
      },
      :body => JSON.dump(body)
    )

    r = JSON.parse(resp.body)
    r.should eq(body)
  end

  it "allows specifying a timeout" do
    resp = Curly::Request.get("#{TEST_URL}/timeout-test",
      :timeout => 1000
    )

    resp.status.should eq(0)
    resp.curl_error.should =~ /timeout/i
  end

end
