<?php 

class Expense extends Storeable
{

    protected function __propertyDefinitions()
    {
        return 
        array(
            "owner" => "int",
            "epoch" => "varchar(255)",
            "comment" => "varchar(255)",
            "description" => "varchar(255)",
            "amount" => "int",
            "deleted" => "int",
            "approved" => "int",
            "touched" => "varchar(255)"
        );    
    }
    
    public function tableName()
    {
        return "Expenses";
    }


    public function mayBeWrittenBy($user)
    {
        return $user->id() == $this->owner();
    }    
    
    public function mayBeApprovedBy($user)
    {
        return $user->mayApproveExpenses();
    }
    
    public function mayBeReadBy($user)
    {
        return $this->mayBeApprovedBy($user) || $this->mayBeWrittenBy($user);        
    }

    public function asTransportableMap()
    {
        $map = Storeable::asTransportableMap();
        $map["approved"] = ($map["approved"] == 1);
        $map["deleted"] = ($map["deleted"] == 1);
        return $map;        
    }

}
?>