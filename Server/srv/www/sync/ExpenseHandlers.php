<?php 

function nowAsTimeStamp()
{
    $mts = new DateTime("now", new DateTimeZone("UTC"));
    return $mts->format('Y-m-d\TH:i:s\Z');        
}

class ExpenseCreator extends JsonReply
{
    
    /** Creates a new expense */
    public function PUT()
    {
        $obj = $this->puttedObject();
        if ( ! isset($obj->amount) )
        {
            throw new BadRequest();            
        } 
        if ( ! isset($obj->epoch) )
        {
            throw new BadRequest();            
        } 
        if ( ! isset($obj->description) )
        {
            throw new BadRequest();            
        } 
        if ( ! isset($obj->comment) )
        {
            $obj->comment = "";            
        } 
        $e = new Expense;
        $e->setEpoch($obj->epoch);
        $e->setComment($obj->comment);
        $e->setDescription($obj->description);
        $e->setAmount($obj->amount);
        $e->setDeleted(false);
        $e->setApproved(false);
        $e->setOwner($this->router()->user()->id());
        $e->setTouched(nowAsTimeStamp());
        $e->save();
       
        $this->setReturnVariable("data", array("id" => $e->id()));
    }    
}

class ExpenseMantainer extends JsonReply
{

/** 
`POST`ing to this endpoint updates an expense where `{N}` is the id of the expense to update. 
    
This endpoint behaves differently depending on what fields are submitted and by whom.

If the field `approved` **is submitted**, only that field and the `touched` field is 
updated iff the user submitting has the privileges to approve expenses. 
It does not matter if other fields are submitted along with the approved filed - 
they are ignored.

If the field 'approved' **is not submitted**, the remaining fields are applied and 
the expense saved if the submitting user has the privileges to write to the ticket.  
However, it the `approved` field is set, no changes to the expense can be done. First you 
must `POST` with `approved=false`. 

The field `deleted` cannot be set via a `POST` retquest. Do a `DELETE` on this endpoint instead


| Attribute     | Mandatory  | Type          | Description                           |
| ------------- |:----------:|---------------| --------------------------------------|
| `id`          |     Yes    | `int`         | The id of the expense to update       |
| `epoch`       |     No     | `timestamp`   | The time in UTC the expense was spend |
| `description` |     No     | `string`      | A description of the expense          |
| `amount`      |     No     | `int`         | The amount in 100th                   |
| `comment`     |     No     | `string`      | An comment                            |
| `approved`    |     No     | `bool`        | Wether this expense has been approved |

If no attributes are provided this is a noop request.

*/
    public function POST($id)
    {
        $expense = Expense::loadById(intval($id));
        if( ! $expense )
        {
            throw new FileNotFound();
        }        
        if($expense->deleted())
        {
            throw new Gone();
        }
        $postdata = $this->postedObject();
        $user = $this->router()->user();
        
        //Maintain approval
        if( isset($postdata->approved) )
        {
            if ( ! $expense->mayBeApprovedBy($user) )
            {
                throw new NotAuthorized();
            }
            $expense->setApproved($postdata->approved);
            $expense->save();
            return;
        }
        
        //Maintain the expense it self
        if ( ! $expense->mayBeWrittenBy($user) )
        {
            throw new NotAuthorized();
        }
        if($expense->approved())
        {
            throw new Conflict();
        }

        
        if(isset($postdata->epoch)) $expense->setEpoch($postdata->epoch);
        if(isset($postdata->description)) $expense->setDescription($postdata->description);
        if(isset($postdata->amount)) $expense->setAmount($postdata->amount);
        if(isset($postdata->comment)) $expense->setComment($postdata->comment);
        
        $expense->save();

    }    


/**
`GET`ting this endpoint returns a single expense where `{N}` is the id of the expense to get.

Anybody who has the privileges to approve and write the expense in question can get this recourse.
*/
    public function GET($id)
    {
        $e = Expense::loadById(intval($id));
        if(! $e)
        {
            throw new FileNotFound();
        }
        if ( ! $e->owner() == $this->router()->user()->id())
        {
            throw new NotAuthorized();
        } 
        
        $this->setReturnVariable("data", array("expense"=>$e->asTransportableMap()));
    }    
    
    
    /** 
Sending a `DELETE` to this endpoing deletes the Expense with id `{N}` 

It important to understand, that a deletion cannot be undone and that once an 
Expense has been deleted, it can no longer be modified. Also, note that expenses 
deleted are not actually removed from the system, but merely flagged as deleted.*/
    public function DELETE($id)
    {
        $expense = Expense::loadById(intval($id));
        if( ! $expense )
        {
            throw new FileNotFound();
        }        

        $user = $this->router()->user();

        if( ! $expense->mayBeWrittenBy($user) )
        {
            throw new NotAuthorized();
        }
        if( $expense->deleted()  == 1 )
        {
            throw new Gone();
        }
        $expense->setDeleted(1);
        $expense->save();
    }    
        
}

class ExpenseLister extends JsonReply
{
    /** `GET`ting this endpoint returns a collection of all Expenses this user is authorized to see.*/
    public function GET()
    {
        
     
        $query = "select Expenses.id, amount, description, comment, owner, epoch, deleted, approved, touched, name,email  from Expenses LEFT JOIN Users on Expenses.owner = Users.id";
        
        $requester =$this->router()->user(); 
        $users = array();
        $expenses = array();
        
        $d = new Database();
        $res = $d->h->query($query);
        $expenses = array();
        for ($row_no = $res->num_rows - 1; $row_no >= 0; $row_no--) {
            $res->data_seek($row_no);
            $row = $res->fetch_assoc();
            
            //Lets filter out the ones we do not want to deliver.
            $add = false;
            if ( $requester->mayApproveExpenses())
            {
                $add = true;
            }
            else
            {
                if( $requester->id() == intval($row["owner"]))
                {
                    $add  =true;
                }
            }
            if ( $add )
            {
                $expenses[] = $row;
            }
        }
        $this->setReturnVariable("data", array("expenses"=>$expenses));
                
    }    
}


?>