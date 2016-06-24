<?php

require_once("SqlUsingTestCase.class.php");
require_once("HttpClient.class.php");


class ApiExpenseTest extends SqlUsingTestCase
{
    
    function testCreateNewExpense()
    {
        $http = new HttpClient;
        $http->doPut("/expenses", array(
            "epoch" => "2016-04-27T14:56:36Z", 
            "comment" => "Because I felt like it....",
            "description" => "An Saturn Rocket",
            "amount" => "1000000"
            ));
        
        $this->assertEquals($http->response()->success, true);
    }


    function dataForTestCreateNewExpenseWithMissingDetails()
    {
        return array(
            array("epoch"),
            array("description"),
            array("amount")
        );
    }

    /**
     * @dataProvider dataForTestCreateNewExpenseWithMissingDetails
     */
    function testCreateNewExpenseWithMissingDetails($keytoremove)
    {
        
        $data =  array(
            "epoch" => "2016-04-27T14:56:36Z", 
            "comment" => "Beacuse I've always wanted one",
            "description" => "A Voyager probe",
            "amount" => "1000000"
            );
            
        unset($data[$keytoremove]);
        $http = new HttpClient;
        $http->doPut("/expenses",$data);
        $this->assertEquals($http->response()->success, false);
    }


    function testCRUDMyOwnExpense()
    {
        //Create an expense
        $data =  array(
            "epoch" => "1957-10-04T19:28:34Z", 
            "comment" => "For the lobby",
            "description" => "Sputnik 1 remake",
            "amount" => "1000000"
            );
            
        $http = new HttpClient;
        $http->doPut("/expenses",$data);
        $this->assertEquals($http->response()->success, true);
        
        $id = $http->response()->data->id;

    
        //Lets fetch it
        $http = new HttpClient;
        $http->doGet("/expenses/$id");
        $this->assertEquals($http->response()->success, true);
        
        
        $expense_as_map =json_Decode(json_encode($http->response()->data->expense), true); 
        $this->assertEquals($expense_as_map["owner"], 2);
        $this->assertEquals($expense_as_map["epoch"], "1957-10-04T19:28:34Z");
        $this->assertEquals($expense_as_map["comment"], "For the lobby");
        $this->assertEquals($expense_as_map["description"], "Sputnik 1 remake");
        $this->assertEquals($expense_as_map["amount"], "1000000");
        $this->assertEquals($expense_as_map["approved"], 0);
        $this->assertEquals($expense_as_map["deleted"], 0);
        $this->assertEquals($expense_as_map["id"], $id);
        
        //Lets update it now.
        $expense_as_map["amount"] = 31415; //We got it a lot cheaper
        unset($expense_as_map["approved"]);
        unset($expense_as_map["deleted"]);
        
        $http = new HttpClient;
        $http->doPost("/expenses/$id", $expense_as_map);
        $this->assertEquals($http->response()->success, true);
        
        
        //Finally, let's check that the amount has been updated.$_COOKIE
        $http = new HttpClient;
        $http->doGet("/expenses/$id");
        $this->assertEquals($http->response()->success, true);
        $expense = $http->response()->data->expense;        
        $this->assertEquals(31415, $expense->amount);
    }
    
    public function helperObjToMap($obj)
    {
        return json_Decode(json_encode($obj), true); 
    }
  
    public function helperCreateExpenseForUserFooBarDotCom()
    {
        //Create an expense
        $data =  array(
            "epoch" => "1957-10-04T19:28:34Z", 
            "comment" => "For the lobby",
            "description" => "Sputnik 1 remake",
            "amount" => "1000000"
            );
            
        $http = new HttpClient;
        $http->doPut("/expenses",$data);
        
        $this->assertEquals($http->response()->success, true);
        
        return $http->response()->data->id;        
    }
  
    public function testCreateAndApproveExpense()
    {
        //Create an expense
        $id = $this->helperCreateExpenseForUserFooBarDotCom();
        
        //The approver fetches it - yes, he can do that.
        $http = new HttpClient;
        $http->setCredentials("exp@approv.er","Boing! Boing!");
        $http->doGet("/expenses/$id");
        $this->assertEquals($http->response()->success, true);

        //Approve it!
        $http = new HttpClient;
        $http->setCredentials("exp@approv.er","Boing! Boing!");
        $http->doPost("/expenses/$id", array("id"=>$id, "approved"=>true));
        $this->assertEquals($http->response()->success, true);

        //Lets check that it has indeed been approved.
        $http = new HttpClient;
        $http->setCredentials("exp@approv.er","Boing! Boing!");
        $http->doGet("/expenses/$id");
        $this->assertTrue($http->response()->data->expense->approved);
        
        //Oh my. The creator wants to increase the amount post approval!
        $http = new HttpClient;
        $http->doPost("/expenses/$id", array("id"=>$id, "amount"=>1000000000));
        $this->assertEquals($http->response()->success, false);
        $this->assertEquals($http->response()->statuscode, 409);
    }
    

    
    public function testCreateAndDeleteExpense()
    {
        //Create an expense
        $id = $this->helperCreateExpenseForUserFooBarDotCom();
        
        //Delete it!
        $http = new HttpClient;
        $http->doDelete("/expenses/$id");
        $this->assertEquals($http->response()->success, true);

        //Lets check that it has indeed been approved.
        $http = new HttpClient;
        $http->doGet("/expenses/$id");
        $this->assertTrue($http->response()->data->expense->deleted);
        
        //Oh my. The creator wants to increase the amount post deletion!
        $http = new HttpClient;
        $http->doPost("/expenses/$id", array("id"=>$id, "amount"=>1000000000));
        $this->assertEquals($http->response()->success, false);
        $this->assertEquals($http->response()->statuscode, 410);
    }


    public function testListAll()
    {
  
        $tocreate = array(
            array("foo@bar.com", "foobar", "1", "Amount 1", "2016-04-27T14:56:36Z"),
            array("foo@bar.com", "foobar", "2", "Amount 2", "2016-04-27T14:56:36Z"),
            array("foo@bar.com", "foobar", "3", "Amount 3", "2016-04-27T14:56:36Z"),
            array("foo@bar.com", "foobar", "4", "Amount 4", "2016-04-27T14:56:36Z")            
        ) ;
        
        foreach($tocreate as $data)
        {
            $h = new HttpClient();
            $h->setCredentials($data[0], $data[1]);
            
            $h->doPut("/expenses", array(
                "epoch" => $data[4], 
                "description" => $data[3],
                "amount" => $data[2]
            ));  
            
                      
        }        
        $http = new HttpClient;
        $http->doGet("/expenses");
        
        $expenses = $http->response()->data->expenses;
        $this->assertTrue(sizeof($expenses) == 30);
        $this->assertEquals("Fubber (pwd 'foobar')", $expenses[0]->name);
                
    }
}

?>