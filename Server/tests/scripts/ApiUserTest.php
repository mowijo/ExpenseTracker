<?php

require_once("SqlUsingTestCase.class.php");
require_once("HttpClient.class.php");

class ApiUserTest extends SqlUsingTestCase
{

    function testCreateNewUser()
    {
        $http = new HttpClient;
        $http->doPut("/users", array("name" => "morten", "email" => "morten@winkler.dk"));
        
        $this->assertEquals($http->response()->success, true);
        $this->assertEquals(is_String($http->response()->data->password), true);
    }

    function testTryToCreateWithTakenEmail()
    {
        $http = new HttpClient;
        $http->doPut("/users", array("name" => "morten", "email" => "morten@winkler.dk"));
        
        $this->assertEquals($http->response()->success, true);
        $this->assertEquals(is_String($http->response()->data->password), true);
        
        $http = new HttpClient;
        $http->doPut("/users", array("name" => "morten", "email" => "morten@winkler.dk"));
        
        $this->assertEquals($http->response()->success, false);
        $this->assertEquals($http->response()->statuscode, 409);        
    }
    
    function testCreateUserAndLogIn()
    {
        //Check we cannot list ourself - cause we do not exist!        
        $http = new HttpClient;
        $http->setCredentials("morten@winkler.dk", "me brute forcing");
        $http->doGet("/users/morten@winkler.dk");
        $this->assertEquals($http->response()->statuscode, 401);        
        
        //Create our self
        $http = new HttpClient;
        $http->doPut("/users",array("name" => "morten", "email" => "morten@winkler.dk"));
        $this->assertEquals($http->response()->success, true);
        $password = $http->response()->data->password;
        
        //Check we cannot list ourself - cause we use wrong password!        
        $http = new HttpClient;
        $http->setCredentials("morten@winkler.dk", "me brute forcing");
        $http->doGet("/users/morten@winkler.dk");
        $this->assertEquals($http->response()->statuscode, 401);       
                
        //Check that we can list ourself.
        $http = new HttpClient;
        $http->setCredentials("morten@winkler.dk", $password);
        $http->doGet("/users/morten@winkler.dk");
        $this->assertEquals($http->response()->success,true);
    }
    
    
    function testUpdateMyself()
    {
        //Create our self
        $http = new HttpClient;
        $http->doPut("/users",array("name" => "morten", "email" => "morten@winkler.dk"));
        $this->assertEquals($http->response()->success, true);
        $password = $http->response()->data->password;
               
        //Check that we can list ourself.
        $http = new HttpClient;
        $http->setCredentials("morten@winkler.dk", $password);
        $http->doGet("/users/morten@winkler.dk");
        
        $user = $http->response()->data->user;
        $id = $user->id;
        $this->assertEquals($user->name, "morten");
        $this->assertEquals($user->email, "morten@winkler.dk");
        $this->assertEquals($user->privileges, 0);

        //Change name and email
        $http = new HttpClient;
        $http->setCredentials("morten@winkler.dk", $password);
        $http->doPost("/users/morten@winkler.dk", array("id" => $user->id, "name"=>"Chewbacca", "email"=>"chew@bacca.com"));
        
        $http = new HttpClient;
        $http->setCredentials("chew@bacca.com", $password);
        $http->doGet("/users/chew@bacca.com");
        $user = $http->response()->data->user;
        
        $this->assertEquals($id, $user->id);
        $this->assertEquals("chew@bacca.com", $user->email);
        $this->assertEquals("Chewbacca", $user->name);
    }    

    function testUpdatingMyPrivileges()
    {
        //Create our self
        $http = new HttpClient;
        $http->doPut("/users",array("name" => "morten", "email" => "morten@winkler.dk"));
        $this->assertEquals($http->response()->success, true);
        $password = $http->response()->data->password;
               
        //Get ourself.
        $http = new HttpClient;
        $http->setCredentials("morten@winkler.dk", $password);
        $http->doGet("/users/morten@winkler.dk");        
        $user = $http->response()->data->user;
        $id = $user->id;
        $this->assertEquals($user->name, "morten");
        $this->assertEquals($user->email, "morten@winkler.dk");
        $this->assertEquals($user->privileges, 0);        
        
        //Try to change privileges  
        $http = new HttpClient;
        $http->setCredentials("morten@winkler.dk", $password);
        $http->doPost("/users/morten@winkler.dk", array("id" => $user->id, "privileges"=>42));
      
        //Verify we are not changed. 
        $http = new HttpClient;
        $http->setCredentials("morten@winkler.dk", $password);
        $http->doGet("/users/morten@winkler.dk");        
        $user = $http->response()->data->user;
        $id = $user->id;
        $this->assertEquals($user->name, "morten");
        $this->assertEquals($user->email, "morten@winkler.dk");
        $this->assertEquals($user->privileges, 0);        
    }    


    function testDeleteUser()
    {
        //Check the user is there
        $http = new HttpClient;
        $http->setCredentials("foo@bar.com", "foobar");
        $http->doGet("/users/foo@bar.com");
        $this->assertTrue( $http->response()->success );

        //Let him try to delete him self.
        $http = new HttpClient;
        $http->setCredentials("foo@bar.com", "foobar");
        $http->doDelete("/users/foo@bar.com");
        $this->assertFalse( $http->response()->success );
        $this->assertEquals( $http->response()->statuscode, 401);
        
        //Let root delete him
        $http = new HttpClient;
        $http->setCredentials("root", "foobar");
        $http->doDelete("/users/foo@bar.com");
        $this->assertTrue( $http->response()->success );
        
        //Check that the user has gone
        $http = new HttpClient;
        $http->setCredentials("foo@bar.com", "foobar");
        $http->doGet("/users/foo@bar.com");
        $this->assertFalse( $http->response()->success );
        $this->assertEquals( $http->response()->statuscode, 401);
    }
    
    function testListUsers()
    {
        $http = new HttpClient;
        $http->setCredentials("root", "foobar");
        $http->doGet("/users");
        $this->assertTrue( $http->response()->success );
        $users = $http->response()->data->users;
        
        $this->assertEquals(sizeof($users), 3);
        foreach($users as $user)
        {
            switch($user->id)
            {
                case 1:
                    $this->assertEquals($user->email,"root");
                    $this->assertEquals($user->name,"Administrator(pwd 'foobar')");
                    $this->assertEquals($user->privileges,2);                
                    break;
                case 2:
                    $this->assertEquals($user->email,"foo@bar.com");
                    $this->assertEquals($user->name,"Fubber (pwd 'foobar')");
                    $this->assertEquals($user->privileges, 0);                
                    break;
                case 3:
                    $this->assertEquals($user->email,"exp@approv.er");
                    $this->assertEquals($user->name,"Approver 'Boing! Boing!')");
                    $this->assertEquals($user->privileges, 1);                
                    break;
                
                default:
                $this->assertTrue(false);
            }
        }
    }

    function testListUsersUnauthorized()
    {
        $http = new HttpClient;
        $http->doGet("/users");
        $this->assertFalse( $http->response()->success );
        
    }
    
    
    function testUpdateUserToAlreadyUsedEmailAddress()
    {
        $usedemail = "foo@bar.com";

        //Verify that a user with said email already exists.
        $http = new HttpClient;
        $http->doGet("/users/$usedemail");
        $this->assertTrue( $http->response()->success );
        
        //Create new user
        $http = new HttpClient;
        $http->doPut("/users", array("name" => "morten", "email" => "morten@winkler.dk"));
        $this->assertEquals($http->response()->success, true);

        //Now, lets get the newly created user - we do this as root...
        $http = new HttpClient;
        $http->setCredentials("root", "foobar");
        $http->doGet("/users/morten@winkler.dk");
        $this->assertEquals($http->response()->success, true);
        $user = $http->response()->data->user;
        
        //Lets change his email to one we know is in use and verify that it fails.
        $user->email = $usedemail;        
        $http = new HttpClient;
        $http->setCredentials("root", "foobar");
        $http->doPost("/users/morten@winkler.dk", $user);
        $this->assertEquals($http->response()->success, false);
        $this->assertEquals($http->response()->statuscode, 409);        

        //Finally, verify that the user has not has his email address changed.
        $http = new HttpClient;
        $http->setCredentials("root", "foobar");
        $http->doGet("/users/morten@winkler.dk");
        $this->assertEquals($http->response()->success, true);


    }
    
    
}



?>