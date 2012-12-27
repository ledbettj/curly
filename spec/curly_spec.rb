require 'spec_helper'
require 'spec_server'
require 'curly'
require 'pry'

TEST_URL = "http://localhost:4567"
describe "Curly::Request" do

  before(:all) do
    SpecServer.start
  end

  after(:all) do
    SpecServer.stop
  end

  let(:params) do
    {
      'x' => 'a',
      'y' => 'b',
      'test' => 'hello world'
    }
  end

  it "returns the appropriate status code" do
    resp = Curly::Request.get("#{TEST_URL}/status-test")

    resp.status.should eq(418)
  end

  it "returns the correct values for success?" do

    resp = Curly::Request.get("#{TEST_URL}/status-test")
    resp.success?.should eq(false)

    resp = Curly::Request.get("#{TEST_URL}/params-test")
    resp.success?.should eq(true)

  end

  it "returns the correct body" do
    resp = Curly::Request.get("#{TEST_URL}/body-test")

    JSON.parse(resp.body).should eq({
        'value' => 1234
      })
  end

  it "handles params passed in the URL" do
    resp = Curly::Request.get("#{TEST_URL}/params-test?x=a&y=b&test=hello+world")

    JSON.parse(resp.body).should eq(params)
  end

  it "handles serializing params without active support" do
    if params.respond_to?(:to_query)
      Hash.send(:remove_method, :to_query)
    end
    resp = Curly::Request.get("#{TEST_URL}/params-test",
      :params => params
    )

    JSON.parse(resp.body).should eq(params)
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
    resp.timed_out?.should eq(true)
  end

end
