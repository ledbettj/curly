require 'spec_helper'
require 'spec_server'
require 'curly'
require 'pry'

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

  it "aliases code to status" do

    resp = Curly::Request.get("#{TEST_URL}/status-test")
    resp.code.should eq(resp.status)
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
      Object.send(:remove_method, :to_query)
    end
    resp = Curly::Request.get("#{TEST_URL}/params-test",
      :params => params
    )

    JSON.parse(resp.body).should eq(params)
  end

  it "handles combination of params in URL and passed in" do

    resp = Curly::Request.get("#{TEST_URL}/params-test?x=a&test=hello+world",
      :params => { 'y' => 'b'}
    )

    JSON.parse(resp.body).should eq(params)
  end

  it "handles setting headers" do
    resp = Curly::Request.get("#{TEST_URL}/headers-test",
      :headers => {
        'X-Example-Header' => 'five',
        'X-Another-Header' => 'abc'
      }
    )

    r = JSON.parse(resp.body)
    r['HTTP_X_EXAMPLE_HEADER'].should eq('five')
    r['HTTP_X_ANOTHER_HEADER'].should eq('abc')

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

  it "can PUT" do
    body = {
      'hello' => 'world',
      'hi' => 3
    }

    resp = Curly::Request.put("#{TEST_URL}/put-test",
      :headers => {
        'Content-Type' => 'application/json'
      },
      :body => JSON.dump(body)
    )

    resp.status.should eq(200)
    JSON.parse(resp.body).should eq(body)
  end

  it "can DELETE" do
    resp = Curly::Request.delete("#{TEST_URL}/delete-test")
    resp.status.should eq(200)
  end

  it "reads headers set by the server" do
    resp = Curly::Request.get("#{TEST_URL}/headers-test")
    resp.headers['X-Sample-Header'].should eq('123')
    resp.headers['X-Another-Header'].should eq('456')
  end

end
