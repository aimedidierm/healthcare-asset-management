<?php

namespace App\Http\Controllers;

use App\Enums\StockMovementType;
use App\Models\StockMovement;
use Illuminate\Http\Request;

class StockMovementController extends Controller
{
    public function all()
    {
        $movements = StockMovement::latest()->with('item')->paginate(10);
        return view('admin.stock.all', compact('movements'));
    }

    public function in()
    {
        $movements = StockMovement::latest()
            ->where('type', StockMovementType::Addition->value)
            ->with('item')
            ->paginate(10);
        return view('admin.stock.in', compact('movements'));
    }

    public function out()
    {
        $movements = StockMovement::latest()
            ->where('type', StockMovementType::Deduction->value)
            ->with('item')
            ->paginate(10);
        return view('admin.stock.out', compact('movements'));
    }

    // public function getStock(int $id)
    // {
    //     $product = Product::findOrFail($id);
    //     return response()->json([
    //         'stock' => $product->current_stock,
    //     ], Response::HTTP_OK);
    // }

    // public function getStockMovements($id)
    // {
    //     $product = Product::findOrFail($id);
    //     $movements = $product->stockMovements()->orderBy('created_at', 'asc')->get();
    //     $movements->load('product.category');

    //     $stockBalance = 0;

    //     foreach ($movements as $movement) {
    //         $stockBalance += $movement->quantity;
    //         $movement->stock_balance = $stockBalance;
    //     }

    //     return response()->json($movements);
    // }

    // public function getAllStockMovements()
    // {
    //     $products = Product::with(['stockMovements' => function ($query) {
    //         $query->orderBy('created_at', 'asc');
    //     }, 'category'])->get();

    //     $result = [];

    //     foreach ($products as $product) {
    //         $stockBalance = 0;

    //         foreach ($product->stockMovements as $movement) {
    //             $stockBalance += $movement->quantity;
    //             $movement->stock_balance = $stockBalance;
    //         }

    //         $result[] = [
    //             'product_id' => $product->id,
    //             'product_name' => $product->name,
    //             'category' => $product->category->name ?? null,
    //             'current_stock_balance' => $product->stockMovements()->sum('quantity'),
    //             'stock_movements' => $product->stockMovements
    //         ];
    //     }

    //     return response()->json($result);
    // }
}
