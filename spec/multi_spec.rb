require 'spec_helper'
require 'spec_server'
require 'curly'
require 'pry'

describe "Curly::Multi" do

  before(:all) do
    SpecServer.start
  end

  after(:all) do
    SpecServer.stop
  end

  it "works with multiple requests" do
    r1 = Curly::Request.new("#{TEST_URL}/status-test")
    r2 = Curly::Request.new("#{TEST_URL}/body-test")

    m = Curly::Multi.new

    m.add(r1)
    m.add(r2)

    m.run

    r1.response.code.should eq(418)

    JSON.parse(r2.response.body).should eq(
      'value' => 1234
    )

  end
end
