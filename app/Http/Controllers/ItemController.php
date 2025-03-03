<?php

namespace App\Http\Controllers;

use App\Http\Requests\ItemRequest;
use App\Models\Item;

class ItemController extends Controller
{
    /**
     * Display a listing of the resource.
     */
    public function index()
    {
        $items = Item::latest()->paginate(10);
        return view('admin.items', compact('items'));
    }

    /**
     * Store a newly created resource in storage.
     */
    public function store(ItemRequest $request)
    {
        Item::create([
            'name' => $request->input('name'),
            'code' => $request->input('code'),
            'unit' => $request->input('unit'),
            'description' => $request->input('description'),
        ]);

        return redirect('/admin/items')->with('success', 'Item added successfully');
    }

    /**
     * Remove the specified resource from storage.
     */
    public function destroy(int $id)
    {
        $item = Item::find($id);

        if ($item) {
            // # code...
        } else {
            # code...
        }
    }
}
