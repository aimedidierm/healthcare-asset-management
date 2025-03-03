<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class Item extends Model
{
    use HasFactory;

    protected $fillable = [
        'name',
        'code',
        'unit',
        'description',
    ];

    public function stockMovements()
    {
        return $this->hasMany(StockMovement::class);
    }

    public function getCurrentStockAttribute()
    {
        return $this->stockMovements()->sum('quantity');
    }
}
