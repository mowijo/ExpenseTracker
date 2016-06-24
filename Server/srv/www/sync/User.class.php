<?php 

class User extends Storeable
{

    const MAY_APPROVE_EXPENSES = 0x01;
    const MAY_MANAGE_USERS     = 0x02;
    
    protected function __propertyDefinitions()
    {
        return 
        array(
            "email" => "varchar(255) unique",            
            "name" => "varchar(255)",
            "password" => "varchar(255)",
            "privileges" => "int"
        );    
    }
    
    public function tableName()
    {
        return "Users";
    }
    
    function setPassword($password)
    {
        $options = [
            'cost' => 10,
        ];
        Parent::setPassword(password_hash($password, PASSWORD_DEFAULT, $options));    
    }
    
    public function matchesPassword($password)
    {
        if ( password_verify ( $password , $this->password() ) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    public function mayView($user)
    {
        if($this->email() == "root") return true;
        return $user->id() == $this->id() ;
    }
        
    public function mayUpdate($user)
    {
        if($this->email() == "root") return true;
        return $user->id() == $this->id() ;
    }
    
    public function mayUpdatePrivileges($user)
    {
        if($this->email() == "root") return true;
        return false;
    }
    
    public function mayDelete($user)
    {
        if($this->email() == "root") return true;
        return false;
    }
    
    public function mayApproveExpenses()
    {
        return ($this->privileges() & self::MAY_APPROVE_EXPENSES);
    }

    public function mayManageUsers()
    {
        //You can never lock out the root user!
        if ( $this->email() == "root") return true;
        return ($this->privileges() & self::MAY_MANAGE_USERS);
    }
    
}

?>